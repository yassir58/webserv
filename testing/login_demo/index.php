<?php
    function redirectTo($location)
    {
        header("Location:". $location);
        exit;
    }
    session_start();
?>
<?php
    if (isset($_SESSION["user"]))
        redirectTo("./account.php");
    if (isset($_POST["submit"]))
    {
        $username = $_POST["username"];
        $password = $_POST["password"];
        if (empty($username) || empty($password))
        {
            $_SESSION["message"] = "All fields must be filled out.";
            redirectTo("./index.php");
        }
        else
        {
            if ($username == "admin" && $password == "home1234")
            {
                $_SESSION["user"] = "admin";
                $cookie_name = "login";
                if (isset($_COOKIE["login"]))
                    $cookie_value = $_COOKIE["login"] + 1;
                else
                    $cookie_value = 1;
                setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/");
                redirectTo("./account.php");
            }
            else
            {
                $_SESSION["message"] = "Wrong username or password";
                redirectTo("./index.php");
            }
        }
    }
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Webserv Login Demo</title>
    <link rel="stylesheet" href="./assets/styles.css">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@100;200;300;400;500;700&display=swap" rel="stylesheet" />
</head>
<body>
    <div class="login_container">
        <img src="./assets/logo.png" class="login_logo" />
        <form action="./index.php" method="POST" class="login_form">
            <input type="text" name="username" placeholder="Username" class="textbox" />
            <input type="password" name="password" placeholder="Password" class="textbox" />
            <button type="submit" name="submit" class="btn">Login</button>
            <p class="text--normal error"><?php echo isset($_SESSION["message"]) ? $_SESSION["message"] : ""; ?></p>
        </form>
    </div>
</body>
</html>
