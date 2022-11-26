/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      backgroundImage: {
        'landing-banner': "url('assets/cgl-site-background-wallpaper.png')",
      }
    },
  },
  plugins: [],
}