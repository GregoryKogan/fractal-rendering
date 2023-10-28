<template>
  <router-view />
</template>

<script lang="ts">
import { defineComponent } from "vue";

export default defineComponent({
  name: "App",
  created() {
    window.addEventListener("message", (event) => {
      let data;

      try {
        data = JSON.parse(event.data);
      } catch (e) {
        return;
      }

      if (!data.to || data.to !== "js") return;
      if (!data.message) return;

      console.log("Message from C++:", data.message);
    });
  },
});
</script>
