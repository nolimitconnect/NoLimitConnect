#!/usr/bin/env bash
unset DEBUGINFOD_URLS
unset DEBUGINFOD_PROGRESS
exec gdb -q -iex "set debuginfod enabled off" "$@"
