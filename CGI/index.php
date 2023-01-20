<?php
    //! The output of running the CGI script should be appended to the response without any seperation or anything.
    echo $_SERVER["QUERY_STRING"];
    echo $_SERVER["SCRIPT_NAME"];
    echo $_SERVER["SERVER_PROTOCOL"];
    echo $_SERVER["SERVER_SOFTWARE"];
    echo $_SERVER["SCRIPT_FILENAME"];
    echo $_SERVER["SERVER_PORT"];

    $cookie_name = "admin";
    $cookie_value = "normal-user";
    setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/");
    var_dump($_COOKIE);
?>
