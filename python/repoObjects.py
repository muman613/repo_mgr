"""
Objects used by reaper
"""

import sqlite3 as sql
import sys
import os


class repoDatabase:
    """
    Class represents the repository database
    """
    def __init__(self, db_path):
        self.db_path = db_path
        if os.path.exists(db_path):
            self.db = sql.connect(db_path)
            curs = self.db.cursor()
            version = curs.execute('SELECT * FROM version')
            (self.maj_ver, self.min_ver, self.ver_date) = version.fetchone()
            self.display_db_version()
        else:
            self.db = sql.connect(db_path)
            curs = self.db.cursor()
            create_db = """
                PRAGMA foreign_keys=OFF;
                BEGIN TRANSACTION;
                CREATE TABLE `version` (
                    `version_major` INTEGER NOT NULL,
                    `version_minor` INTEGER NOT NULL,
                    `version_date`  TEXT NOT NULL
                );
                CREATE TABLE repos (branch varchar, subbranch varchar, checkout varchar, owner varchar, purpose varchar, category varchar, type varchar);
                CREATE INDEX repos_index on repos(branch asc,subbranch asc);
                COMMIT;
            """
            print("Creating new repository database...")
            curs.executescript(create_db)
            version_tuple = (1, 3, "2018-04-14")
            (self.maj_ver, self.min_ver, self.ver_date) = version_tuple
            curs.execute("INSERT INTO version VALUES(?,?,?)", version_tuple)
            curs.execute("COMMIT")
            self.display_db_version()

    def __del__(self):
        if hasattr(self, 'db'):
            self.db.close()

    def display_db_version(self):
        """
        Display the database version information.
        :return:
        """
        print("Database file @ {}".format(self.db_path))
        print("Database version {}.{} generated on {}".format(self.maj_ver, self.min_ver, self.ver_date))

    def add_repository(self, branch, subbranch, checkout, owner, purpose, category, type):
        """
        Add a new repository to the database.
        :param branch:
        :param subbranch:
        :param checkout:
        :param owner:
        :param purpose:
        :param category:
        :param type:
        :return:
        """
        with self.db as curs:
            curs.execute("INSERT INTO repos VALUES(?,?,?,?,?,?,?)",
                         (branch, subbranch, checkout, owner, purpose, category, type))

