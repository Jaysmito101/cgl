import Container from "react-bootstrap/esm/Container";

export default function Content() {
  return (
    <div id="Content">
      <Container>
        <br/>
        <p align="center">
            <img src="https://raw.githubusercontent.com/Jaysmito101/cgl/main/logo.png" border="0" alt="CGL Logo" />
        </p>
        <span id="intro-text">
          <p>CGL (C Graphics Library) is a multipurpose library mainly used for rendering graphics.</p>
          <p>It has many uses in recreational coding / demo scenes / prototyping / small games / experimentation. </p>
          <p>Best of all all of it is inside a single header file cgl.h. Also CGL is made purely in C but is also compatible with C++.</p>
          <p>:-)</p>
        </span>
        
      </Container>
    </div>
  );
}