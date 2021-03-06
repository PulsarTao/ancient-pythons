# Cache lines from files.
# This is intended to read lines from modules imported -- hence if a filename
# is not found, it will look down the module search path for a file by
# that name.

import sys
import os
from stat import *

def getline(filename, lineno):
	lines = getlines(filename)
	if 1 <= lineno <= len(lines):
		return lines[lineno-1]
	else:
		return ''


# The cache

cache = {} # The cache


# Clear the cache entirely

def clearcache():
	global cache
	cache = {}


# Get the lines for a file from the cache.
# Update the cache if it doesn't contain an entry for this file already.

def getlines(filename):
	if cache.has_key(filename):
		return cache[filename][2]
	else:
		return updatecache(filename)


# Discard cache entries that are out of date.
# (This is not checked upon each call!)

def checkcache():
	for filename in cache.keys():
		size, mtime, lines, fullname = cache[filename]
		try:
			stat = os.stat(fullname)
		except os.error:
			del cache[filename]
			continue
		if size <> stat[ST_SIZE] or mtime <> stat[ST_MTIME]:
			del cache[filename]


# Update a cache entry and return its list of lines.
# If something's wrong, print a message, discard the cache entry,
# and return an empty list.

def updatecache(filename):
	if cache.has_key(filename):
		del cache[filename]
	if not filename or filename[0] + filename[-1] == '<>':
		return []
	fullname = filename
	try:
		stat = os.stat(fullname)
	except os.error, msg:
		# Try looking through the module search path
		basename = os.path.split(filename)[1]
		for dirname in sys.path:
			fullname = os.path.join(dirname, basename)
			try:
				stat = os.stat(fullname)
				break
			except os.error:
				pass
		else:
			# No luck
			print '*** Cannot stat', filename, ':', msg
			return []
	try:
		fp = open(fullname, 'r')
		lines = fp.readlines()
		fp.close()
	except IOError, msg:
		print '*** Cannot open', fullname, ':', msg
		return []
	size, mtime = stat[ST_SIZE], stat[ST_MTIME]
	cache[filename] = size, mtime, lines, fullname
	return lines
