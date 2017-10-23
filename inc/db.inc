<?php

include "base62.inc";

//
// Database configuration
//
define('DB_SERVER', '192.168.95.134');
define('DB_USERNAME', 'root');
define('DB_PASSWORD', 'mysql');
define('DB_DATABASE', 'test');
define('DB_TABLE', 'TblURL');


//
// Time zone configuration
//
define('DB_TIMEZONE', 'Asia/Kuala_Lumpur');


//
// Logging level
//
define('DB_LOG_NONE', -1);
define('DB_LOG_ERROR', 0);
define('DB_LOG_INFO',  1);
define('DB_LOG_DEBUG', 2);


//
// Global variable
//
$connection = NULL;
$logLevel = DB_LOG_DEBUG;


// Initialize time zone
setTimeZone(DB_TIMEZONE);


//
// Logging for database module
//
function dbLog($level, $input) {

  global $logLevel;

  $levelStr = array("ERROR", "INFO", "DEBUG");

  if ($level <= $logLevel) {

    // Logging to standard output, i.e. browser
    printf("%s [DB] %s: %s<br>", date("M d Y H:i:s"), $levelStr[$level], $input);

    // Logging to default log file, i.e. /var/log/httpd/error_log
    error_log("[DB] " . $levelStr[$level] . ": " . $input, 0);

  }

}


//
// Set the timezone
//
function setTimeZone($tz) {

  date_default_timezone_set($tz);

  dbLog(DB_LOG_INFO, "Time zone set to " . $tz);

}


//
// Connect to server and database 
//
function connectDB() {

  global $connection;

  $connection = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD);

  if (mysqli_connect_errno()) {

    dbLog(DB_LOG_ERROR, "Failed to connect to MySQL: " . mysqli_connect_error());
    return FALSE;

  }

  dbLog(DB_LOG_INFO, "Connected to server");

  if (mysqli_select_db($connection, DB_DATABASE)) {

    dbLog(DB_LOG_INFO, "Connected to " . DB_DATABASE . " database");
    return TRUE;

  } else {

    dbLog(DB_LOG_ERROR, "Failed to connect to " . DB_DATABASE . " database");
    return FALSE;

  }
}


// 
// createTable() is hardcoded here. Would be better if it is made a generic
// function which can create tables dynamically by reading the schema from
// an array/structure, external xml/ini/conf configuration, etc
//
function createTable() {

  global $connection;

  // Check if this table already exists
  $query = "SELECT TABLE_NAME FROM information_schema.TABLES " .
             "WHERE TABLE_NAME = '" . DB_TABLE . "' AND " .
             "TABLE_SCHEMA = '" . DB_DATABASE . "'";

  dbLog(DB_LOG_DEBUG, "SELECT query: " . $query);

  $result = mysqli_query($connection, $query);

  if (mysqli_num_rows($result) == 0) {

    mysqli_free_result($result);

    $query = "CREATE TABLE `" . DB_TABLE . "` (" .
               "`Id` int(10) NOT NULL AUTO_INCREMENT, " .
               "`ShortUrl` varchar(10) DEFAULT NULL, " .
               "`LongUrl` varchar(5000) DEFAULT NULL, " .
               "PRIMARY KEY (`Id`), " .
               "UNIQUE KEY `ID_UNIQUE` (`Id`)" .
               ") ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1";

    dbLog(DB_LOG_DEBUG, "CREATE query: " . $query);

    if (mysqli_query($connection, $query)) {

      dbLog(DB_LOG_INFO, "Table " . DB_TABLE . " created");

    } else {

      dbLog(DB_LOG_ERROR, "Failed to create table " . DB_TABLE);
      return FALSE;

    }

    // Set the auto-increment to start from 1,000,000,001 
    // to generate "long enough" base62 string
    $query = "ALTER TABLE `" . DB_TABLE . "` AUTO_INCREMENT=1000000001";

    dbLog(DB_LOG_DEBUG, "ALTER query: " . $query);

    if (mysqli_query($connection, $query)) {

       dbLog(DB_LOG_INFO, "Table " . DB_TABLE . " altered");
       return TRUE;

    } else {

       dbLog(DB_LOG_ERROR, "Failed to alter table " . DB_TABLE);
       return FALSE;

    }

  } else {

    mysqli_free_result($result);
    dbLog(DB_LOG_INFO, "Table " . DB_TABLE  . " already exist");
    return TRUE;

  }
}


//
// Shorten the given URL by base62 encoding the auto-incremented record index
//
function shortenUrl($longUrl) {

  global $connection;

  // Start DB transaction
  mysqli_autocommit($connection, FALSE);

  // Save the URL in database
  $url = mysqli_real_escape_string($connection, $longUrl);
  $query = "INSERT INTO `TblURL` (`LongURL`) VALUES ('$url')";

  dbLog(DB_LOG_DEBUG, "longUrl: " . $longUrl);
  dbLog(DB_LOG_DEBUG, "url   : " . $url);
  dbLog(DB_LOG_DEBUG, "INSERT query: " . $query);

  if (mysqli_query($connection, $query)) {

    // Get the auto-incremented ID from newly added record
    $lastId = mysqli_insert_id($connection);
    
    dbLog(DB_LOG_DEBUG, "lastId: " . $lastId);

    // Encode the base62 string from the ID
    $shortUrl = base62encode($lastId);

    dbLog(DB_LOG_DEBUG, "shortUrl: " . $shortUrl);

    //Update the base62 string into the newly added record
    $query = "UPDATE `TblURL` SET `ShortURL` = '$shortUrl' WHERE `Id` = '$lastId'";

    dbLog(DB_LOG_DEBUG, "UPDATE query: " . $query);

    if (mysqli_query($connection, $query)) {

      // Commit DB transaction
      if (mysqli_commit($connection)) {

        dbLog(DB_LOG_INFO, "URL added into database: " . $url);
        dbLog(DB_LOG_INFO, "Id: " . $lastId . ", ShortURL: " . $shortUrl);

        dbLog(DB_LOG_DEBUG, "shortUrl decoded back to: " . base62decode($shortUrl));

        mysqli_close($connection);

        return $shortUrl;

      } else {

        dbLog(DB_LOG_ERROR, "Fail to commit URL: " . $url);

        mysqli_close($connection);

        return -1;

      }

    } else {

      // Rollback DB transaction
      mysqli_rollback($connection);

      dbLog(DB_LOG_ERROR, "Failed to update URL: " . $url);

      mysqli_close($connection);

      return -1;

    }

  } else {

    dbLog(DB_LOG_ERROR, "Failed to add URL: " . $url);

    mysqli_close($connection);

    return -1;

  }
}


//
// Retrieve the original URL based on the given short URL
//
function lengthenUrl($shortUrl) {

  global $connection;

  $id = base62decode($shortUrl);

  $query = "SELECT * FROM `TblURL` WHERE `Id` = '$id' AND `ShortUrl` = '$shortUrl'";

  dbLog(DB_LOG_DEBUG, "SELECT query: " . $query);

  $result = mysqli_query($connection, $query);

  $row = mysqli_fetch_array($result, MYSQLI_ASSOC);

  mysqli_free_result($result);

  mysqli_close($connection);

  if ($row) {
    
    dbLog(DB_LOG_INFO, "Record retrieved: " .
                         "[Id=" . $row["Id"] . "] " .
                         "[ShortUrl=" . $row["ShortUrl"] . "] " .
                         "[LongUrl=" . $row["LongUrl"] . "]");

    return $row["LongUrl"];

  } else {

    dbLog(DB_LOG_ERROR, "Failed to retrieve original URL for " .
                          "[Id=" . $id . "] " .
                          "[ShortUrl=" . $shortUrl . "]");

    return "";

  }
}

?>
