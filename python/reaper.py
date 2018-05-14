"""
Python implementation of reaper
"""

# import sqlite3 as sql
# import pygit2
import sys
# import os
from repoObjects import repoDatabase

repo_path = "..\\database\\repositories.db"


print("module = {}".format(__name__))
print("Running on {}".format(sys.platform))
print("Python version {}".format(sys.version))


repo = repoDatabase(repo_path)
repo.dump_rows()

newrepo = repoDatabase(".\\test.db")
if newrepo.add_repository('main', '', 'git clone ssh://gitmaster@raspi-mate/gitroot/repos/master.git',
                       'muman', 'main master branch', 'dev', 'git'):
    print("Repository added!")
else:
    print("Unable to add repository...")

newrepo.dump_rows()
