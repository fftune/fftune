#include "midi_file.hpp"

namespace fftune {

#define MIDI_NOTE_OFF 0x80
#define MIDI_NOTE_ON 0x90

midi_event::midi_event() {
}

midi_event::midi_event(int note, double clock, int velocity) {
	this->note = note;
	this->clock = clock;
	this->velocity = velocity;
}

smf_event_t *midi_event::to_smf(bool note_on) const {
	if (note_on) {
		return smf_event_new_from_bytes(MIDI_NOTE_ON, note, velocity);
	} else {
		// velocity does not matter for note-off
		return smf_event_new_from_bytes(MIDI_NOTE_OFF, note, 127);
	}
}


midi_file::midi_file() {
	smf = smf_new();
	track = smf_track_new();
	smf_add_track(smf, track);
}

midi_file::~midi_file() {
	smf_track_delete(track);
	smf_delete(smf);
}

bool midi_file::write(const std::filesystem::path &out_file) {
	flush();
	return !smf_save(smf, out_file.c_str());
}

void midi_file::flush() {
	std::ranges::for_each(pending_events, [&](const auto& ev){ flush_event(ev); });
	pending_events.clear();
}

void midi_file::add_notes(note_estimates notes, double duration) {
	constexpr float confidence_threshold = 0.6f;
	for (auto note : notes) {
		if (!note.valid()) {
			// not on piano
			continue;
		}
		if (note.confidence < confidence_threshold) {
			// if confidence is low, we assume that the last note repeats
			// note = pending_event.note;
			// TODO: Actually keep the last note
			continue;
		}
		auto ev = std::ranges::find_if(pending_events, [&](const auto& e){ return e.note == note.note; });
		if (ev == pending_events.cend()) {
			// we didn't have this note playing already
			// cool, let's find all the notes that were previously on and are now not on anymore
#ifdef __clang__
			// clang does not yet support ranges filter
			std::vector<midi_event> orphaned_notes;
			for (auto i : pending_events) {
				if (std::ranges::none_of(notes, [&](const auto& n){ return i.note == n.note; })) {
					orphaned_notes.push_back(i);
				}
			}
#else
			// all notes that were played previously but are not played anymore
			auto orphaned_notes = pending_events | std::ranges::views::filter([&](const auto& ev){ return std::ranges::none_of(notes, [&](const auto& n){ return ev.note == n.note; }); });
#endif
			midi_event new_note = {note.note, clock, note.velocity};
			if (orphaned_notes.empty()) {
				// no orphans available, create a new voice for the new note
				pending_events.push_back(new_note);
			} else {
				// lead the nearest voice to the new note
				auto nearest_note = std::ranges::min_element(orphaned_notes, [&](const auto& a, const auto& b){ return std::abs(a.note - note.note) < std::abs(b.note - note.note); });
				flush_event(*nearest_note);
				// recreate as new note
				*nearest_note = new_note;
			}
		}
		// else the note was already playing so we can just keep everything as is
	}
	clock += duration;
}

void midi_file::flush_event(const midi_event &event) {
	auto *ev = event.to_smf();
	smf_track_add_event_seconds(track, ev, event.clock);
	ev = event.to_smf(false);
	smf_track_add_event_seconds(track, ev, clock);
}

}
