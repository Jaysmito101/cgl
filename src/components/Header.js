import Navbar from 'react-bootstrap/Navbar';
import Container from 'react-bootstrap/Container';
import { Form, Button, Nav } from 'react-bootstrap';

export default function Header() {
  return ( 
    <Navbar bg="dark" variant="dark" sticky="top" expand="lg">
      <Container fluid>
        <Navbar.Brand href="#" className="site-heading">CGL</Navbar.Brand>

        <Navbar.Toggle aria-controls="navbarScroll" />
        
        <Navbar.Collapse id="navBarScroll">
          <Nav className="me-auto my-2 my-lg-0">
            <Nav.Link href="#" style={{ color: 'white' }}>Docs</Nav.Link>
            <Nav.Link href="#" style={{ color: 'white' }}>Examples</Nav.Link>
          </Nav>

          <Form className="d-flex">
            <Form.Control 
              type="search"
              placeholder="# TODO: search"
              className="header-searchbar me-2"
            />
            <Button variant="success">Search</Button>
          </Form>
        </Navbar.Collapse>
      </Container>
    </Navbar>
  );
}