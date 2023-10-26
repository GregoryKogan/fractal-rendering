<template>
  <v-btn @click="restart">restart</v-btn>
</template>

<script lang="ts">
import { defineComponent } from "vue";

export default defineComponent({
  name: "ReattachCanvasButton",
  methods: {
    restart() {
      let oldScript = document.getElementById("restart-sdl-canvas");
      if (oldScript) oldScript.remove();

      let script = document.createElement("script");
      script.type = "text/javascript";
      script.text = `if (Module) Module.callMain();`;
      script.id = "restart-sdl-canvas";
      document.body.appendChild(script);
    },
  },
});
</script>
