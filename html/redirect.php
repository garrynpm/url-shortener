<?php

  include "../inc/db.inc";

  // Get short URL for processing
  $shortUrl = htmlentities($_GET['url']);

  if (!connectDB()) {

    echo("Failed to connect to database. Please check server log.<br><br>");
    exit();

  }

  $url = lengthenUrl($shortUrl);

  if ($url == "") {

    echo("Failed to retrieve the original URL. Please check server log.<br><br>");
    exit();

  }

  // Check if the long URL starts with http. If not add the default http://
  // There are other "schemes", e.g. ftp, mailto, file, data, irc., but not handled here
  // Redirect only http and https here

  if (strncasecmp($url, "http", 4) != 0)
    $url = "http://" . $url;

  printf("URL redirection to %s<br><br>", $url);

  header("Location: " . $url);
  exit();

?>
