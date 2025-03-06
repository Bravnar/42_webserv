<!DOCTYPE html>
<html lang="en">
<head>
    <link rel="icon" href="./winnie_php.jpg">
    <link rel="stylesheet" href="./style.css">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP-CGI!</title>
</head>
<body>
    <section>
        <div class="container">
            <h1>CGI-PHP Calculator</h1>
            <h2>Hope you're having a great day!</h2>
        </div>
        <div class="container">
            <h1>Welcome to PHP CGI Page!</h1>
            <div style="margin-top: 0px;" class="container">
		<h2>Results</h2>
<?php
    if (!isset($_COOKIE["histo"]))
    {
        setcookie("histo", json_encode(array()), time() + (86400 * 30), "/");
        $histo = array();
    }
    else
    {
        $histo = json_decode($_COOKIE["histo"]);
    }


    if (isset($_GET["nbr1"]) && isset($_GET["nbr2"]) && isset($_GET["operation"]))
    {
        $nbr1 = intval($_GET["nbr1"]);
        $nbr2 = intval($_GET["nbr2"]);
        $op = $_GET["operation"];
        $res = 0;

        switch($op)
        {
            case '+':
                $res = $nbr1 + $nbr2;
                break;

            case '-':
                $res = $nbr1 - $nbr2;
                break;

            case '*':
                $res = $nbr1 * $nbr2;
                break;

            case '/':
                if ($nbr2 != 0)
                {
                    $res = $nbr1 / $nbr2;
                }
                else
                {
                    $res = "impossible";
                }
                break;

            default:
                $res = "invalid operation";
        }


        array_push($histo, "PHP GET : " . $nbr1 . " " . $op . " " . $nbr2 . " is " . $res);
        $histo = array_slice($histo, max(0, count($histo) - 10));

        setcookie("histo", json_encode($histo), time() + (86400 * 30), "/");

        echo ("calcul made with get" . "<br>");
        echo ("result of " . $nbr1 . " " . $op . " " . $nbr2 . " is " . $res . " !!!! " . "<br>");

    }
    else if (isset($_POST["nbr1"]) && isset($_POST["nbr2"]) && isset($_POST["operation"]))
    {
        $nbr1 = intval($_POST["nbr1"]);
        $nbr2 = intval($_POST["nbr2"]);
        $op = $_POST["operation"];
        $res = 0;

        switch($op)
        {
            case '+':
                $res = $nbr1 + $nbr2;
                break;

            case '-':
                $res = $nbr1 - $nbr2;
                break;

            case '*':
                $res = $nbr1 * $nbr2;
                break;

            case '/':
                if ($nbr2 != 0)
                {
                    $res = $nbr1 / $nbr2;
                }
                else
                {
                    $res = "impossible";
                }
                break;

            default:
                $res = "invalid operation";
        }

        array_push($histo, "PHP POST : " . $nbr1 . " " . $op . " " . $nbr2 . " is " . $res);
        $histo = array_slice($histo, max(0, count($histo) - 10));

        setcookie("histo", json_encode($histo), time() + (86400 * 30), "/");

        echo ("calcul made with post" . "<br>");
        echo ("result of " . $nbr1 . " " . $op . " " . $nbr2 . " is " . $res . " !!!! " . "<br>");

    }

    


?>



<br>
<br>

<form>
</form>

<br>

<h2>Historique</h2>

<?php 
foreach ($histo as $calc)
{
    echo ($calc. "<br>");
}

?>
            </div>
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
</body>

</html>