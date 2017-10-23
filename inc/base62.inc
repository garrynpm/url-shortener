<?php

//
// Global variables for base62 encoding/decoding
//

// Using base62
$baseNum = 62;

// The [0-9A-Za-z] sequence of string below in $baseStr is not randomized
// $baseStr = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
// The $baseStr below is randomized, but not as good as a hashed string
$baseStr = "jWbZCc0hixLe4mNrgqkMBlS6KVUu8J9YODAH3z5RoPXydEaIQG2F1pnT7fvwst";


//
// Functions for base62 encoding/decoding
//

function base62encode($input) {

  global $baseStr, $baseNum;

  $output = "";

  while ($input > 0) {

    $remainder = $input % $baseNum;
    $input = (int)($input / $baseNum);
    $output .= $baseStr[$remainder];

  }

  return strrev($output);

}


function base62decode($input) {

  global $baseStr, $baseNum;

  $output = 0;
  $len = strlen($input);

  for ($i = 0; $i < $len; $i++) {

    $idx = strpos($baseStr, $input[$i]);
    $output = $baseNum * $output + $idx;

  }

  return $output;

}

?>
