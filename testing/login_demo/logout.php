<?php
    function redirectTo($location)
    {
        header("Location:". $location);
        exit;
    }
    session_start();
    session_destroy();
    redirectTo("./index.php");
?>