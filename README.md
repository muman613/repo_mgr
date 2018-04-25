# repo_mgr repository

## Overview

This repository contains the sourcecode for two utilities which simplify software development by making git easier to use.

### reaper

`reaper` is a tool which keeps track of git repositories and branches in an `sqlite3` database. Using easy to remember labels reaper can use git, repo, or the included crepo tools to perform the checkout.

Look out for the EASTER EGG:

    reaper -z

### crepo

Much like repo, crepo allows several git repositories to be checked out into different directories, driven by a manifest XML file.
