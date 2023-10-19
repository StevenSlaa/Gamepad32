const char page[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        html {
            font-family: Arial, Helvetica, sans-serif;
            text-align: center;
        }
    </style>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
</head>

<body>
    <div class="content">
        <pre id="gamepad-data"></pre>
    </div>
    <script>
        let gamepad;
        let gamepadButtonStates = [];
        let gamepadAxisValues = [];

        // Add an event listener for when a gamepad is connected.
        window.addEventListener("gamepadconnected", (e) => {
            gamepad = e.gamepad;
            console.log(
                "Gamepad connected at index %d: %s. %d buttons, %d axes.",
                e.gamepad.index,
                e.gamepad.id,
                e.gamepad.buttons.length,
                e.gamepad.axes.length
            );

            // Start sending gamepad data over the WebSocket.
            initWebSocket();
        });

        let url = "%IP%";
        // var gateway = `ws://${window.location.hostname}/ws`;
        var gateway = `ws://${url}/ws`;
        var websocket;

        const gamepadDataDisplay = document.getElementById("gamepad-data");

        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            websocket = new WebSocket(gateway);
            websocket.onopen = onOpen;
            websocket.onclose = onClose;
            websocket.onmessage = onMessage;
        }

        function onOpen(event) {
            console.log('Connection opened');
            sendGamepadData();
        }

        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }

        function onMessage({ data }) {
            if (data.charAt(0) === 'R') {
                let values = data.substring(1).split('.')
                console.log(values)
                gamepad.vibrationActuator.playEffect("dual-rumble", {
                    duration: values[0],
                    startDelay: values[1],
                    weakMagnitude: values[2]/2000,
                    strongMagnitude: values[3]/2000,
                });
            }
        }

        function sendGamepadData() {
            const convertInput = (value) => Math.round(value * 1000) + 1000;
            const itterateInput = (inputArray, compareArray, prefix) => {
                for (let i = 0; i < inputArray.length; i++) {
                    if (compareArray[i] !== convertInput(inputArray[i])) {
                        compareArray[i] = convertInput(inputArray[i]);
                        websocket.send(`${prefix}${i}.${convertInput(inputArray[i])}`);
                    }
                }
            }
            for (const gamepad of navigator.getGamepads()) {
                if (!gamepad) continue;
                itterateInput(gamepad.buttons.map((btn) => btn.value), gamepadButtonStates, 'B');
                itterateInput(gamepad.axes, gamepadAxisValues, 'A');
                gamepadDataDisplay.textContent = JSON.stringify({ buttons: gamepadButtonStates, axes: gamepadAxisValues }, null, 2);
            }
            requestAnimationFrame(sendGamepadData);
        }
    </script>

</body>

</html>
)rawliteral";