const client = mqtt.connect('wss://broker.hivemq.com:8884/mqtt');

const mqttTopics = [
  "imperialeeet4/sensors/infra_red",
  "imperialeeet4/sensors/magnetic",
  "imperialeeet4/sensors/radio"
];

client.on("connect", () => {
  console.log("MQTT connected!");
  for (const topic of mqttTopics) {
    client.subscribe(topic);
  }
})

client.on("disconnect", (_) => {
  console.log("MQTT disconnected!!");
})

client.on("message", (topic, message) => {
  const value = message.toString();
  console.log(topic, value);
})


const buttons = {
  fwd: { press: "w", release: "x" },
  bwd: { press: "s", release: "t" },
  left: { press: "a", release: "b" },
  right: { press: "d", release: "e" },
};

for (const [key, value] of Object.entries(buttons)) {
  const btn = document.getElementById(`btn-${key}`);

  function sendDirectionToggle(cancel) {
    if (cancel) {
      client.publish("imperialeeet4/move", value.release);
    } else {
      client.publish("imperialeeet4/move", value.press);
    }
  }

  function toggleUIState(event, state) {
    if (event == null || event.key === value.press) {
      if (state === true) {
        btn.classList.add("active");
      } else {
        btn.classList.remove("active");
      }
    }
  }

  btn.addEventListener("mousedown", (_) => {
    toggleUIState(null, true);
    sendDirectionToggle();
  })

  btn.addEventListener("mouseup", (_) => {
    toggleUIState(null, false);
    sendDirectionToggle(true);
  })

  window.addEventListener("keydown", (event) => {
    toggleUIState(event, true);
    if (event.key === value.press && !event.repeat) {
      sendDirectionToggle();
    }
  })

  window.addEventListener("keyup", (event) => {
    toggleUIState(event, false);
    if (event.key === value.press) {
      sendDirectionToggle(true);
    }
  })
}