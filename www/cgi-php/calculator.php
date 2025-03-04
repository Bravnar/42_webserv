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

Historique : 
<br>

<?php 
foreach ($histo as $calc)
{
    echo ($calc. "<br>");
}

?>