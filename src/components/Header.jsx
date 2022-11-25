import React from "react";

export default function Header() {
  return (
    <>
      <nav className="container mx-auto py-2.5 px-5 mt-2 rounded-xl bg-gray-900 w-full z-20 border-gray-600">
        <div className="container flex flex-wrap mx-auto items-center justify-between">

          <div className="pb-3 pl-4">
            <span className="self-center ml-5 mb-3">
              <img src="/src/assets/cgl-logo-2.png" alt="CGL Logo"/>
            </span>
          </div>

          <div className="flex items-center w-auto">
            <ul className="flex flex-row space-x-10 p-4 mt-4 text-3xl pb-6">
              <li>
                <a href="#" className="block pl-2 px-3 text-gray-500 hover:text-gray-100">Examples</a>
              </li>
              <li>
                <a href="#" className="block pl-2 px-3 text-gray-500 hover:text-gray-100">Documentation</a>
              </li>
            </ul>
          </div>

          <div> 
            <a href="https://github.com/Jaysmito101/cgl/">
              <button type="submit" className="text-white bg-blue-600 hover:bg-blue-700 px-5 mr-5 py-3 ml-2 text-2xl rounded-lg">View in Github</button>
            </a>
          </div>

          {/* <form>
            <div className="relative">
              <input type="search" id="search" className="w-auto p-4 py-3 text-sm border border-gray-300 rounded-lg bg-gray-700 focus:border-blue-500 text-white" placeholder="Query..." required/>
              <button type="submit" className="text-white bg-blue-600 hover:bg-blue-700 px-2 py-3 ml-2 rounded-lg">Search!</button>
            </div>
          </form> */}
        </div>
      </nav>
    </>
  );
}