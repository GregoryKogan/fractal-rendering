<template>
  <v-container
    style="
      height: 100%;
      justify-content: center;
      align-items: center;
      display: flex;
      flex-direction: column;
    "
  >
    <h1 style="text-align: center">SDL2 Canvas & Vue 3</h1>
    <div class="canvas-wrapper"><SDLCanvas /></div>
    <v-col>
      <v-row style="width: 100%; justify-content: space-evenly">
        <v-btn @click="sendMessage">MESSAGE</v-btn>
        <ReattachCanvasButton />
      </v-row>
    </v-col>
  </v-container>
</template>

<script lang="ts">
import { defineComponent } from "vue";
import SDLCanvas from "@/components/SDLCanvas.vue";
import ReattachCanvasButton from "@/components/ReattachCanvasButton.vue";

export default defineComponent({
  name: "Home",
  components: {
    SDLCanvas,
    ReattachCanvasButton,
  },
  methods: {
    sendMessage() {
      window.postMessage(
        JSON.stringify({
          to: "cpp",
          message: { type: "message", payload: "Hello, world!" },
        }),
        "*"
      );
    },
  },
});
</script>

<style scoped>
.canvas-wrapper {
  width: 100%;
  margin: 1em;
  aspect-ratio: 16 / 9;
  border: solid 2px rgb(var(--v-theme-primary));
}
</style>
