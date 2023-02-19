<?php
    function redirectTo($location)
    {
        header("Location:". $location);
        exit;
    }
    session_start();
?>
<?php
    if (!isset($_SESSION["user"]))
        redirectTo("index.php");
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Account</title>
    <link rel="stylesheet" href="./assets/styles.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.2.1/css/all.min.css">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@100;200;300;400;500;700&display=swap" rel="stylesheet" />
</head>
<body>
    <div class="user_container">
        <img src="./assets/logo.png" class="login_logo" />
        <div class="account_container">
            <i class="fa-solid fa-user icon"></i>
            <p class="text--normal">You're successfully logged in as <?php echo $_SESSION["user"] ?>, you've logged in <?php echo $_COOKIE["login"] ?> times since the application has started.</p>
            <a href="./logout.php" class="btn">Logout</a>
        </div>
    </div>
</body>
</html>