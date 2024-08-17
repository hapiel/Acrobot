import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import { ThemeProvider } from '@/components/theme-provider.tsx';
import { createBrowserRouter, RouterProvider } from 'react-router-dom';
import { Root } from '@/routes/root.tsx';
import { ErrorPage } from '@/routes/error-page';
import { RobotStatus } from '@/routes/robot-status';

const router = createBrowserRouter([
  {
    path: '/',
    element: <Root />,
    errorElement: <Root />,
    children: [
      { index: true, element: <RobotStatus /> },
      { path: '*', element: <ErrorPage /> }
    ]
  }
]);

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <ThemeProvider defaultTheme="dark" storageKey="acrobot-dashboard-theme">
      <RouterProvider router={router} />
    </ThemeProvider>
  </React.StrictMode>
);
