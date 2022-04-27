-- Application "chat" is about web chatting between application users.
-- "chat" application, lies on the "chat" database were user data,
-- user relationships, user messages etc are stored.

-- (Re)create the "chat" database.

DROP DATABASE IF EXISTS `chat`
;

CREATE DATABASE `chat`
DEFAULT CHARSET = utf8mb4
DEFAULT COLLATE = utf8mb4_general_ci
;

-- Select "chat" database as the default database.

USE `chat`
;

-- Table "user" is the most significant table of the "chat" database.
-- Each row represents a registered user of the "chat" application.

CREATE TABLE `user` (
	`login`		VARCHAR(64) NOT NULL COMMENT 'Login name',
	`registration`	TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'Registration date',
	`name`		VARCHAR(128) NOT NULL COMMENT 'Full name',
	`email`		VARCHAR(128) NULL DEFAULT NULL COMMENT 'e-mail',
	`gender`	ENUM (
		'MALE',
		'FEMALE'
	) NULL DEFAULT NULL COMMENT 'Gender',
	`byear`		INTEGER(4) NULL DEFAULT NULL COMMENT 'Birth year',
	-- 		user passwords are stored in SHA1
	`password`	CHARACTER(40) COLLATE utf8_bin NOT NULL COMMENT 'Password',

	PRIMARY KEY (
		`login`
	) USING BTREE,

	INDEX (
		`name`
	) USING BTREE,

	UNIQUE INDEX (
		`email`
	) USING BTREE
)

ENGINE = InnoDB
COMMENT = 'Person table'
;

-- Table "relation" holds the relationships between users.
-- There are three kinds of relationships between any two users:
-- The users are unrelated, friends or blocked.

CREATE TABLE `relation` (
	`user`		VARCHAR(64) NOT NULL COMMENT 'Login name',
	`related`	VARCHAR(64) NOT NULL COMMENT 'Related login name',
	`relationship`	ENUM (
		'FRIEND',
		'BLOCKED'
	) NOT NULL COMMENT 'Relationship',

	PRIMARY KEY (
		`user`,
		`related`
	) USING BTREE,

	INDEX (
		`related`
	) USING BTREE
)

ENGINE = InnoDB
COMMENT = 'Relation table'
;

-- Table "message" holds the messages between users.

CREATE TABLE `message` (
	`id`		INTEGER(9) NOT NULL AUTO_INCREMENT COMMENT 'Message ID',
	`sender`	VARCHAR(64) NOT NULL COMMENT 'Sender',
	`recipient`	VARCHAR(64) NOT NULL COMMENT 'Recipient',
	`date`		TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'Date',
	`text`		VARCHAR(10000) NOT NULL COMMENT 'Text',

	PRIMARY KEY (
		`id`
	) USING BTREE,

	INDEX (
		`sender`
	) USING BTREE,

	INDEX (
		`recipient`
	) USING BTREE
)

ENGINE = InnoDB
COMMENT = 'Message table'
;

ALTER TABLE `relation` ADD FOREIGN KEY (
	`user`
) REFERENCES `user` (
	`login`
) ON UPDATE CASCADE ON DELETE CASCADE
;

ALTER TABLE `relation` ADD FOREIGN KEY (
	`related`
) REFERENCES `user` (
	`login`
) ON UPDATE CASCADE ON DELETE CASCADE
;

ALTER TABLE `message` ADD FOREIGN KEY (
	`sender`
) REFERENCES `user` (
	`login`
) ON UPDATE CASCADE ON DELETE CASCADE
;

ALTER TABLE `message` ADD FOREIGN KEY (
	`recipient`
) REFERENCES `user` (
	`login`
) ON UPDATE CASCADE ON DELETE CASCADE
;

-- Create user for generic DQL/DML access to "chat" database.

DROP USER IF EXISTS 'chat'@'localhost'
;

CREATE USER 'chat'@'localhost' IDENTIFIED BY 'xxx'
;

GRANT SELECT, INSERT, UPDATE, DELETE ON `chat`.* TO 'chat'@'localhost'
;

-- Create user for generic DQL access to "chatdql" database.

DROP USER IF EXISTS 'chatdql'@'localhost'
;

CREATE USER 'chatdql'@'localhost'
;

GRANT SELECT ON `chat`.* TO 'chatdql'@'localhost'
;

-- Create user for DDL/DQL/DML access to "chat" database.

DROP USER IF EXISTS 'chatadmin'@'localhost'
;

CREATE USER 'chatadmin'@'localhost' IDENTIFIED BY 'xxx'
;

GRANT ALL PRIVILEGES ON `chat`.* TO 'chatadmin'@'localhost'
;
