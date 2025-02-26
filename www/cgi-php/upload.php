#!/usr/bin/php-cgi
<?php

$upload_dir = rtrim(getenv("UPLOAD_DIR_PHP") ?: "./www/uploads", "/");

echo $upload_dir ;

if (!file_exists($upload_dir)) {
	mkdir($upload_dir, 0777, true) ;
}

function	get_unique_filename($directory, $filename) {
	$path_info = pathinfo($filename) ;
	$base = $path_info['filename'] ;
	$ext = isset($path_info['extension']) ? "." . $path_info['extension'] : "" ;
	$counter = 1 ;
	$new_filename = $filename ;

	while (file_exists($directory . "/" . $new_filename)) {
		$new_filename = "{$base}_{$counter}{$ext}";
		$counter++ ;
	}

	return $new_filename ;
}

if (!empty($_FILES['uploaded_file']['name'])) {
	$filename = basename($_FILES['uploaded_file']['name']) ;
	$filename = get_unique_filename($upload_dir, $filename) ;
	$file_path = $upload_dir . "/" . $filename ;


	if (move_uploaded_file($_FILES['uploaded_file']['tmp_name'], $file_path)) {
		$content = "
		<!DOCTYPE html>
		<html lang=\"en\">
			<body>
				<h2>File '{$filename}' uploaded successfully!</h2>
				<p>Saved at: {$file_path}</p>
				<a href=\"/cgi-php/\">Go Back.</a>
			</body>
		</html>
		";
	} else {
		$content = "
        <!DOCTYPE html>
        <html lang=\"en\">
        <body>
            <h2>Error uploading file!</h2>
            <a href=\"/cgi-php/\">Go Back.</a>
        </body>
        </html>
        ";
	} 
} else {
	$content = "
    <!DOCTYPE html>
    <html lang=\"en\">
    <body>
        <h2>No file uploaded.</h2>
        <a href=\"/cgi-php/\">Go Back.</a>
    </body>
    </html>
    ";
}

header("Content-Type: text/html");
header("Content-Length: " . strlen($content));

echo $content;
?>