#!/usr/bin/php-cgi
<?php
// pre responses
$successContent = "
<!DOCTYPE html>
<html lang=\"en\">
<head>
	<link rel=\"icon\" href=\"/winnie.jpg\">
	<link rel=\"stylesheet\" href=\"./upload_assets/style.css\">
    <meta charset=\"UTF-8\">
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
    <title>WebservSIR</title>
</head>
<body>
	<section>
		<div class=\"container\">
			<h1>Hunny received!</h1>
			<p>We received your {filename} hunny</p>
			<img src=\"./upload_assets/hunny.png\" alt=\"Placeholder Image\">
			<hr>
			<p>Take a look at uploaded files</p>
			<div class=\"buttons cgi-buttons\">
				<a href=\"./\">go back</a>
				<a href=\"./uploads/\">/uploads</a>
			</div>
		</div>
	</section>
</body>
</html>
";

$errorContent = "
<!DOCTYPE html>
<html lang=\"en\">
<head>
	<link rel=\"icon\" href=\"/winnie.jpg\">
	<link rel=\"stylesheet\" href=\"./uploaded_assets/style.css\">
    <meta charset=\"UTF-8\">
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
    <title>WebservSIR</title>
</head>
<body>
	<section>
		<div class=\"container\">
			<h1>Oh noo!</h1>
			<p>An error occured while uploading the file!</p>
			<img src=\"./uploaded_assets/sad_winnie.jpg\" alt=\"Placeholder Image\">
			<hr>
			<p>Take a look at uploaded files</p>
			<div class=\"buttons cgi-buttons\">
				<a href=\"./\">go back</a>
				<a href=\"./uploads/\">/uploads</a>
			</div>
		</div>
	</section>
</body>
</html>";

// code
$upload_dir = rtrim(getenv("UPLOAD_DIR_PHP") ?: "./www/uploads", "/");

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
		$content = str_replace("{filename}", $filename, $successContent);
	} else {
		$content = $errorContent;
	} 
} else {
	$content = $errorContent;
}

header("Content-Type: text/html");
header("Content-Length: " . strlen($content));

echo $content;
?>