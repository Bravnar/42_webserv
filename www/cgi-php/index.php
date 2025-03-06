#!/usr/bin/php-cgi
<!DOCTYPE html>
<html lang="en">
<head>
    <link rel="icon" href="./winnie_php.jpg">
    <link rel="stylesheet" href="./style.css">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PhP-CGI!</title>
</head>
<body>
    <section>
        <div class="container">
            <h1>WebServ-PHP-CGI</h1>
            <h2>Happy <span style="color: blue;">
                <?php
                $day_of_week = date("l");
                $day_message = "$day_of_week";
                echo $day_message;
                ?>!</span> Hope you're having a great day!</h2>
        </div>
        <div class="container">
            <h1>Welcome to PHP CGI Page!</h1>
            <img src="./winnie_php.jpg" alt="Image Placeholder">
            <br>
            <p>The main website is at <strong>http://127.0.0.1/</strong></p>
            <p>But this one is at <strong>http://127.0.0.1/cgi-php/index.php</strong>!</p>
            <div class="buttons">
                <a href="/">Go To Main Site</a>
                <a href="/404notfound">Get a 404 Error!</a>                
            </div>
            <div class="buttons cgi-buttons">
                <a href="../cgi-python/">PYTHON CGI</a>
                <a href="../cgi-bash/">BASH CGI</a>
                <a href="../cgi-php/calculator.html">CGI Calculator!</a>
            </div>
        </div>
    </section>
    <section>
        <div class="container">
            <h2>Upload Memes</h2>
            <p1>( *.png, *.jpg, *.jpeg, *.gif )</p1>
            <div class="container" style="margin-top: 20px; max-width: 40%;">
                <h3>Upload a File</h3>
                <br>
                <form action="./upload.php" method="post" enctype="multipart/form-data">
                    <input type="file" name="uploaded_file" id="fileToUpload">
                    <input type="submit" value="Upload">
                </form>
            </div>
            <h2 style="margin-top: 50px;">Latest Memes</h2>
            <div style="display: flex; width: 100%; justify-content: center; align-items: center; flex-direction: row; gap: 20px; flex-wrap: wrap;">
                <?php
                $dir = './uploads';
                $files = scandir($dir);
                foreach ($files as $file) {
                    if ($file !== '.' && $file !== '..' && preg_match('/\.(png|jpg|jpeg|gif)$/i', $file)) {
                        $creationDate = date("F d Y H:i:s.", filectime('./uploads/' . $file));
                        echo '
                        <div class="container picture" style="display: flex; flex-direction: column; align-items: center; margin: 10px;">
                            <img src="./uploads/' . $file . '" alt="Image Placeholder" style="max-width: 100%; height: auto;">
                            <p>' . $file . '</p>
                            <p>Created on: ' . $creationDate . '</p>
                            <a href="#" onclick="deleteFile(\'' . $file . '\')" style="background-color: #f44336; color: white; border: none; padding: 10px; cursor: pointer; text-decoration: none; margin-top: 10px;">Delete</a>
                        </div>';
                    }
                }                
                ?>
            </div>
        </div>
    </section>
    <script>
        function deleteFile(fileName) {
            var xhr = new XMLHttpRequest();
            xhr.open('DELETE', './uploads/' + fileName, true);
            xhr.onload = function() {
                if (xhr.status === 200 || xhr.status === 204) {
                    location.reload();
                } else {
                    alert('Failed to delete file.');
                }
            };
            xhr.send();
        }
    </script>
</body>
</html>