#!/usr/bin/env sh

git ls-files| grep -E '.*\.[ch]pp'| xargs clang-format -style=file -i
STATUS="$(git status -s)"
if [ -n "$STATUS" ]; then
	git --no-pager diff && false
fi
