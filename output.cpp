//Must output the website with no external refs, just plain html...
//Use % % % (with no spaces) to represent a line converted, it will be repeated
#include "output.h"

void outputSite(void (*printFunction)(const char *) ) {
	printFunction("<!doctype html>");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("<html lang=\"en\">");
	printFunction("<head> ");
	printFunction("    <meta charset=\"utf-8\">");
	printFunction("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
	printFunction("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
	printFunction("    <title>CRW System Monitor</title>");
	printFunction("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
	printFunction("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"//cdn.rawgit.com/Mikhus/canvas-gauges/gh-pages/download/2.1.5/radial/gauge.min.js\"></script>");
	printFunction("");
	printFunction("    <script>");
	printFunction("        function refresh() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                console.log(data);");
	printFunction("                if (data.moving > 0) {");
	printFunction("                    animateOn();");
	printFunction("                } else {");
	printFunction("                    animateOff();");
	printFunction("                }");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function timerHit() {");
	printFunction("            loadIt();");
	printFunction("            logButton();");
	printFunction("        }");
	printFunction("");
	printFunction("        function loadIt() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                //$(\"#dWater\").html(\"Water:\" + data.Water + \" Inches\");");
	printFunction("                gauges[0].value = data.Temp;");
	printFunction("                //$(\"#tempGauge\").attr('value', data.Temp);");
	printFunction("                //$(\"#dWind\").html(\"Wind: \" + data.Wind + \" mph\");");
	printFunction("                gauges[1].value = (data.Water);");
	printFunction("                gauges[2].value = (data.Time);");
	printFunction("");
	printFunction("                //$(\"#dWater\").html(\"Water:\" + data.Water + \" Inches\");");
	printFunction("                $(\"#dPump\").html(\"Cooler Pump:\" + data.Pump);");
	printFunction("                $(\"#dFan\").html(\"Cooler Fan:\" + data.Fan);");
	printFunction("                $(\"#dThermo\").html(\"Thermo:\" + data.ThermostatCool); //ThermostatCool");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function logButton() {");
	printFunction("            $.get(\"/log.json\", function (data) {");
	printFunction("                $(\"#dLog\").html(data); ");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        $(document).ready(function () {");
	printFunction("            timerHit();");
	printFunction("            setInterval(timerHit, 5000);");
	printFunction("            $(\"#bLog\").click(logButton);");
	printFunction("        });");
	printFunction("");
	printFunction("    </script>");
	printFunction("");
	printFunction("</head>");
	printFunction("<!-- test -->");
	printFunction("<body>");
	printFunction("    <div class=\"container\">");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            <h1>CRW Water Monitoring</h1>");
	printFunction("            <h2>OTA Upload 2019-06-18</h2>");
	printFunction("            <h3>No more CString</h3>");
	printFunction("        </div>");
	printFunction("        <div class=\"row\">");
	printFunction("            <div class=\"col\" id=\"dTemp\">");
	printFunction("                <canvas id=\"tempGauge\"");
	printFunction("                        data-type=\"radial-gauge\"");
	printFunction("                        data-width=\"300\"");
	printFunction("                        data-height=\"300\"");
	printFunction("                        data-units=\"Temp F\"");
	printFunction("                        data-min-value=\"-40\"");
	printFunction("                        data-max-value=\"120\"");
	printFunction("                        data-valueDec=\"1\"");
	printFunction("                        data-major-ticks=\"-40,-20,0,20,40,60,80,100,120\"");
	printFunction("                        data-minor-ticks=\"2\"");
	printFunction("                        data-stroke-ticks=\"true\"");
	printFunction("                        data-highlights='[");
	printFunction("        {\"from\": 80, \"to\": 120, \"color\": \"rgba(200, 50, 50, .75)\"},");
	printFunction("        {\"from\": -20, \"to\": 50, \"color\": \"rgba(50, 50,200, .75)\"}");
	printFunction("");
	printFunction("    ]'");
	printFunction("                        data-color-plate=\"#fff\"");
	printFunction("                        data-border-shadow-width=\"0\"");
	printFunction("                        data-borders=\"false\"");
	printFunction("                        data-needle-type=\"arrow\"");
	printFunction("                        data-needle-width=\"2\"");
	printFunction("                        data-needle-circle-size=\"7\"");
	printFunction("                        data-needle-circle-outer=\"true\"");
	printFunction("                        data-needle-circle-inner=\"false\"");
	printFunction("                        data-animation-duration=\"1500\"");
	printFunction("                        data-animation-rule=\"linear\"></canvas>");
	printFunction("            </div>");
	printFunction("            <div class=\"col\" id=\"dWater\">");
	printFunction("                <canvas id=\"windGauge\"");
	printFunction("                        data-type=\"radial-gauge\"");
	printFunction("                        data-width=\"300\"");
	printFunction("                        data-height=\"300\"");
	printFunction("                        data-units=\"Water Gallons\"");
	printFunction("                        data-min-value=\"0\"");
	printFunction("                        data-max-value=\"175\"");
	printFunction("                        data-valueDec=\"1\"");
	printFunction("                        data-major-ticks=\"0,25,50,75,100,125,150,175\"");
	printFunction("                        data-minor-ticks=\"5\"");
	printFunction("                        data-stroke-ticks=\"true\"");
	printFunction("                        data-highlights='[");
	printFunction("        {\"from\": 0, \"to\": 55, \"color\": \"rgba(200, 50, 50, .75)\"},");
	printFunction("        {\"from\": 55, \"to\": 95, \"color\": \"rgba(200, 50,50, .50)\"},");
	printFunction("        {\"from\": 95, \"to\": 175, \"color\": \"rgba(50, 200,50, .25)\"}]'");
	printFunction("                        data-color-plate=\"#fff\"");
	printFunction("                        data-border-shadow-width=\"0\"");
	printFunction("                        data-borders=\"false\"");
	printFunction("                        data-needle-type=\"arrow\"");
	printFunction("                        data-needle-width=\"2\"");
	printFunction("                        data-needle-circle-size=\"7\"");
	printFunction("                        data-needle-circle-outer=\"true\"");
	printFunction("                        data-needle-circle-inner=\"false\"");
	printFunction("                        data-animation-duration=\"1500\"");
	printFunction("                        data-animation-rule=\"linear\"></canvas>");
	printFunction("            </div>");
	printFunction("");
	printFunction("            <div class=\"col\" id=\"dTime\">");
	printFunction("                <canvas id=\"timeGauge\"");
	printFunction("                        data-type=\"radial-gauge\"");
	printFunction("                        data-width=\"300\"");
	printFunction("                        data-height=\"300\"");
	printFunction("                        data-units=\"Time\"");
	printFunction("                        data-min-value=\"0\"");
	printFunction("                        data-max-value=\"24\"");
	printFunction("                        data-valueDec=\".01\"");
	printFunction("                        data-major-ticks=\"0,3,6,9,12,15,18,21,24\"");
	printFunction("                        data-minor-ticks=\"1\"");
	printFunction("                        data-stroke-ticks=\"true\"");
	printFunction("                        data-color-plate=\"#fff\"");
	printFunction("                        data-border-shadow-width=\"0\"");
	printFunction("                        data-borders=\"false\"");
	printFunction("                        data-needle-type=\"arrow\"");
	printFunction("                        data-needle-width=\"2\"");
	printFunction("                        data-needle-circle-size=\"7\"");
	printFunction("                        data-needle-circle-outer=\"true\"");
	printFunction("                        data-needle-circle-inner=\"false\"");
	printFunction("                        data-animation-duration=\"250\"");
	printFunction("                        data-animation-rule=\"linear\"");
	printFunction("                        data-highlights='[");
	printFunction("        {\"from\": 0, \"to\": 5.15, \"color\": \"rgba(50, 50, 200, .75)\"},");
	printFunction("");
	printFunction("        {\"from\": 19.45, \"to\": 24, \"color\": \"rgba(50,50,200,.75)\"}]'></canvas>");
	printFunction("            </div>");
	printFunction("        </div>");
	printFunction("");
	printFunction("        <div class=\"row text-center\">");
	printFunction("            <div class=\"col\" id=\"dPump\">Pump</div>");
	printFunction("            <div class=\"col\" id=\"dFan\">Fan</div>");
	printFunction("            <div class=\"col\" id=\"dThermo\">Thermo</div>");
	printFunction("        </div>");
	printFunction("");
	printFunction("        <br><br>");
	printFunction("        <button type=\"button\" class=\"btn\" id=\"bLog\">System Log</button>");
	printFunction("        <div>Log File</div>");
	printFunction("        <div id=\"dLog\"></div>");
	printFunction("");
	printFunction("        <br />");
	printFunction("        <div id=\"dRaw\"></div>");
	printFunction("    </div>");
	printFunction("</body>");
	printFunction("</html>");
}