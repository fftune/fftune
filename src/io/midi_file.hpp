#pragma once

#include <string>
#include <array>

extern "C" {
#include <smf.h>
}

#include "util/music.hpp"

namespace fftune {

/**
 * @brief A single Midi event
 *
 * This class provides an abstraction over a Midi event.
 */
class midi_event {
public:
	/**
	 * @brief Constructs a midi_event
	 */
	midi_event();
	/**
	 * @brief Constructs a midi_event with predefined values
	 *
	 * The Midi event is initialized with the given parameters
	 */
	midi_event(int note, double clock, int velocity);
	/**
	 * @brief Converts a midi_event to smf
	 *
	 * This will return an equivalent smf_event_t *
	 */
	smf_event_t *to_smf(bool note_on = true) const;

	// TODO: Maybe add a midi_t typedef

	/**
	 * @brief The note of the Midi event
	 *
	 * This stores the note, that is played or stopped.
	 * For example an \c A4 has the value \c 69.
	 */
	int note = 0;
	/**
	 * @brief The time of the Midi event
	 *
	 * This stores the time in seconds.
	 */
	double clock = 0;
	/**
	 * @brief The velocity of the Midi event
	 *
	 * This stores the volume of the note, for a noteon event.
	 */
	int velocity = 80;
};


/**
 * @brief An abstraction over a Midi file
 *
 * This file can be stored to disk.
 */
class midi_file {
public:
	/**
	 * @brief Constructs a Midi file
	 *
	 * Creates an empty Midi file
	 */
	midi_file();
	/**
	 * @brief Destructs a Midi file
	 *
	 * Deallocates all data
	 */
	~midi_file();
	/**
	 * @brief Writes the Midi file to disk
	 *
	 * This will write the Midi file to \p out_file
	 */
	bool write(const std::filesystem::path &out_file);
	/**
	 * @brief Flushes all Midi events
	 *
	 * Any Midi events that are still pending, will be flushed upon calling this function.
	 */
	void flush();
	/**
	 * @brief Adds notes to the Midi file
	 *
	 * This will add the given \p notes, and advance the clock by \p duration
	 */
	void add_notes(note_estimates notes, double duration);
private:
	void flush_event(const midi_event &event);

	smf_t *smf;
	smf_track_t *track;
	double clock = 0.0;
	std::vector<midi_event> pending_events;
};

}
