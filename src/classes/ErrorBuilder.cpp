#include "./ErrorBuilder.hpp"

ErrorBuilder::ErrorBuilder() {}
ErrorBuilder::ErrorBuilder(const ErrorBuilder& copy) {(void)copy;}
ErrorBuilder& ErrorBuilder::operator=(const ErrorBuilder& assign) {(void)assign; return *this;}
ErrorBuilder::~ErrorBuilder() {}

std::string ErrorBuilder::buildBody(const HttpResponse& response) {
	std::string body = "\
<!DOCTYPE html> \
<html lang=\"en\"> \
<head> \
    <link rel=\"icon\" href=\"/winnie.jpg\"> \
    <link rel=\"stylesheet\" href=\"./style.css\"> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>" + Convert::ToString(response.getStatus()) + " " + response.getStatusMsg() + "</title> \
	<style> \
		* { \
			margin: 0; \
			padding: 0; \
			box-sizing: border-box; \
			font-family: Arial, sans-serif; \
		} \
		body { \
			background-color: #fff; \
			color: #000; \
		} \
		section { \
			width: 100%; \
			min-height: 80vh; \
			display: flex; \
			flex-direction: column; \
			align-items: center; \
		} \
		.container { \
			margin-top: 100px; \
			max-height: fit-content; \
			width: 60%; \
			border: 2px solid black; \
			padding: 30px; \
			text-align: center; \
			box-shadow: 5px 5px 0px black; \
			display: flex; \
			flex-direction: column; \
			justify-content: center; \
			align-items: center; \
		} \
		.container.picture { \
			width: 30%; \
			height: 20%; \
			margin: 0; \
			padding: 10px; \
			display: inline; \
		} \
		h1 { \
			border-bottom: 2px solid black; \
			padding-bottom: 10px; \
			margin-bottom: 20px; \
		} \
		img { \
			width: 100%; \
			max-width: 400px; \
			height: auto; \
			border: 2px solid black; \
			margin: 20px 0; \
		} \
		p { \
			line-height: 1.6; \
		} \
		.buttons { \
			margin-top: 20px; \
		} \
		.buttons a { \
			display: inline-block; \
			text-decoration: none; \
			color: black; \
			border: 2px solid black; \
			padding: 10px 20px; \
			margin: 10px; \
			font-size: 16px; \
		} \
		.buttons a:hover { \
			background: black; \
			color: white; \
		} \
 \
		.cgi-buttons a { \
			display: inline-block; \
			text-decoration: none; \
			color: black; \
			background-color: rgb(107, 107, 219); \
			border: 2px solid black; \
			padding: 10px 20px; \
			margin: 10px; \
			font-size: 16px; \
		} \
 \
		.cgi-buttons a:hover { \
			background: #022449; \
			color: white ; \
		} \
	</style> \
</head> \
<body> \
    <section> \
        <div class=\"container\"> \
            <h1>" + Convert::ToString(response.getStatus()) + " ERROR</h1> \
			<p>" + response.getStatusMsg() + "</p> \
        </div> \
        <div class=\"container\"> \
            <h1>Oh noooooo!</h1> \
			<div style=\"width: 100%;\"\"> \
            	<img src=\"data:image/png;base64,/9j/4AAQSkZJRgABAQEASABIAAD//gBMRmlsZSBzb3VyY2U6IGh0dHBzOi8vZW4ubWVtaW5nLndv \
				cmxkL3dpa2kvRmlsZTpXaW5uaWVfdGhlX1Bvb2hfUmVhZGluZy5qcGf/2wBDAAYEBQYFBAYGBQYH \
				BwYIChAKCgkJChQODwwQFxQYGBcUFhYaHSUfGhsjHBYWICwgIyYnKSopGR8tMC0oMCUoKSj/2wBD \
				AQcHBwoIChMKChMoGhYaKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgo \
				KCgoKCgoKCj/wAARCADnASwDASIAAhEBAxEB/8QAHAAAAgIDAQEAAAAAAAAAAAAAAAQDBQIGBwEI \
				/8QAQxAAAQMCAwUFBQUGBQQDAQAAAQACAwQRBRIhBjFBUWETInGBkQcUMqHBI0JSYrEVM0NygtEW \
				JJKy4QhTovBUg6PC/8QAGwEAAgMBAQEAAAAAAAAAAAAAAAECBAUDBgf/xAAzEQACAgEDAgMGBAYD \
				AAAAAAAAAQIDEQQhMQUSE0FRIjJhcaHwFCOx4RVCgZHB0QYz8f/aAAwDAQACEQMRAD8A525mYNOu \
				470NYMt7HTmpQDmseC9c27SLFcQyQMZncN9ipX0j7XabhSRx9+O3NPNBy9UCyVYppgLgXWBim7eN \
				+VwGrT57le07TmN7JswhzNwvvCaQu4pHNcWtFupWIhDG5bajXxVnLCBJyudLLLEYQYS8fEBuHFDQ \
				ZKuL4fmFBiLM0J48vVNRizGkgagFR1v4QOFrIQCIiLW6iwWYhzxjMdMwJWbpWxQl8otYXI3+XVW1 \
				Bs/NUQB9dI+mzi4hjtmbf8Tjx6Bcrr4UrM2da6Z2vESsoYScz/xG4vwCsGM0524p5+z7omXo62US \
				AaCZocw9DYAjxS1G97y+GpiMVVEftI73A5EHiDzUadVXe8QYX6eyreSIiyw03qVgJAFtVO2K7tyk \
				cA07lZKwnM1trEX6qte37Sw0CuJRcWsq2RtpHW3pkkyBwDNXEBLwxmeoBv3b5zpw4D6oqn3LWE2D \
				t/gN6sqBtoA4ixf3j05BRGzJkZdZoCzdTODDfRORBoF//Ssqhriy/TcmQya1UDvtABBBsk6WLK2Q \
				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\" style=\"margin: 0; padding: 0;\"> \
				<p style=\"position: relative; bottom: 160px; left: 90px; height: 0px; font-size: x-large;\">" + Convert::ToString(response.getStatus()) + "<br>" + response.getStatusMsg() + "</p> \
			</div> \
            <br> \
            <p>Looks like you've stumbled into the void...</p> \
            <p>Maybe you were looking for:</p> \
            <div class=\"buttons\"> \
                <a href=\"/\">Go Home</a> \
                <a href=\"./ddanceparty\">Dance the pain away!</a> \
                <a href=\"https://www.youtube.com/watch?v=dQw4w9WgXcQ\" target=\"_blank\">Secret Link?</a> \
            </div> \
        </div> \
    </section> \
</body> \
</html> \
";
	return body;
}