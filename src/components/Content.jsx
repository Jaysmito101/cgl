import React from 'react';

import Banner from './Banner';

export default function Content() {
  return (
    <div
      class="relative overflow-hidden bg-no-repeat bg-cover"
      style={{
        backgroundPosition: "50%",
        backgroundImage: "url('/src/assets/cgl-site-background-wallpaper.png')",
        height: "500px",
      }}
    >
      <Banner />
      <p>Content goes here.</p>
    </div>
  );
}
