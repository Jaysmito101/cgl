import Container from 'react-bootstrap/Container';
// import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';

// Custom component imports
import Header from './components/Header';
import Content from './components/Content';
import Footer from './components/Footer';

export default function App() {
  return (
    <Container fluid className="App">
      <Header />
      <Content />
      <Footer />
    </Container>
  );
}

