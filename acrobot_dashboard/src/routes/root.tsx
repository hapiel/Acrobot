import { Navbar } from '@/components/navbar';
import { Outlet } from 'react-router-dom';

export function Root() {
  return (
    <>
      <Navbar />
      <Outlet />
    </>
  );
}
