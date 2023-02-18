<?php
    session_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Upload Test</title>
    <link rel="stylesheet" href="styles.css">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@100;200;300;400;500;700&display=swap" rel="stylesheet" />
</head>
<body>
        <h1 class="upload_header">Upload file webserv/1.0</h1>
        <form action="upload.php" method="post" enctype="multipart/form-data" class="upload_form">
            <div>
              <label for="file">Choose file to upload:</label>
              <input type="file" id="file" name="file" />
              <input type="submit" value="Upload" class="btn" name="submit">
              <p class="error"><?php echo isset($_SESSION["message"]) ? $_SESSION["message"] : ""; ?></p>
              <p class="success"><?php echo isset($_SESSION["success"]) ? $_SESSION["success"] : ""; ?></p>
            </div>
        </form>
</body>
</html>