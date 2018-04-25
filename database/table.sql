PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE repos (branch varchar, subbranch varchar, checkout varchar, owner varchar, purpose varchar, category varchar, type varchar);
INSERT INTO "repos" VALUES('Product Main Dev Branch','','repo init -u ssh://dev_branch/manifest.git -g all -b prod_main',NULL,NULL,'prod','repo');
CREATE INDEX repos_index on repos(branch asc,subbranch asc);
COMMIT;
