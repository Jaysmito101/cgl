import React from 'react';

export default function Banner() {
  return (
    <>
      <div class="text-center bg-gray-50 text-gray-800 py-20 px-6" style={{backgroundColor: "rgba(0, 0, 0, 0.75)"}}>
        <h1 class="text-5xl text-gray-500 font-bold mt-0 mb-6">CGL</h1>
        <h3 class="text-3xl text-gray-500 font-bold mb-8">Creative Coding with C</h3>
        <a class="inline-block px-6 py-2.5 bg-blue-600 text-white font-medium text-xs leading-tight uppercase rounded shadow-md hover:bg-blue-700 hover:shadow-lg focus:bg-blue-700 focus:shadow-lg focus:outline-none focus:ring-0 active:bg-blue-800 active:shadow-lg transition duration-150 ease-in-out" data-mdb-ripple="true" data-mdb-ripple-color="light" href="#!" role="button">Get started</a>
      </div>
    </>
  );
}
