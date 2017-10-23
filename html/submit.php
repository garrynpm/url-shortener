<html>
<body>

<?php

  include "../inc/db.inc";

  // Get user entered URL for processing

  // htmlentities() converts & to &amp; in the URL, so avoid it for now
  // $oriURL = htmlentities($_GET['oriURL']);

  $oriURL = $_GET['oriURL'];
  echo("Submitted URL is: " . $oriURL . "<br><br>");

  if (!connectDB()) {

    echo("Failed to connect to database. Please check server log.<br><br>");
    exit();

  }

  if (!createTable()) {

    echo("Database table failure. Please check server log.<br><br>");
    exit();

  }

  $shortUrl = shortenUrl($oriURL);

  if ($shortUrl == -1) {

    echo("Failed to shorten the URL. Please check server log.<br><br>");

  } else {

    $shortUrl = "http://$_SERVER[HTTP_HOST]/" . $shortUrl;
    printf("Shortened URL is: <a href=\"%s\">%s</a><br><br>", $shortUrl, $shortUrl);

  }

?>

</body>
</html>
