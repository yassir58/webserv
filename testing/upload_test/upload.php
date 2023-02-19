<?php
    session_start();
?>
<?php
    $file = basename($_FILES["file"]["name"]);
    if(isset($_POST["submit"])) {
        if (file_exists($file)) {
            $_SESSION["message"] = "Sorry, file already exists.";
            $_SESSION["success"] = "";
            header("Location:./index.php");
            exit;
        }
        if ($_FILES["file"]["size"] > 500000) {
            $_SESSION["message"] = "Sorry, your file is too large.";
            $_SESSION["success"] = "";
            header("Location:./index.php");
            exit;
        }
        if (move_uploaded_file($_FILES["file"]["tmp_name"], $file)) {
            $_SESSION["success"] = "The file ". htmlspecialchars( basename( $_FILES["file"]["name"])). " has been uploaded.";
            $_SESSION["message"] = "";
            header("Location:./index.php");
            exit;
        } else {
            $_SESSION["message"] = "Sorry, there was an error uploading your file.";
            $_SESSION["success"] = "";
            header("Location:./index.php");
            exit;
        }
    }
?>