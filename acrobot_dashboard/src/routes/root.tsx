import { Footer } from '@/components/footer';
import { Navbar } from '@/components/navbar';
import { QueryClient, QueryClientProvider } from 'react-query';
import { Outlet } from 'react-router-dom';

const queryClient = new QueryClient();

export function Root() {
  return (
    <QueryClientProvider client={queryClient}>
      <div className="relative flex h-screen flex-col">
        <main className="flex flex-1 flex-col overflow-hidden pb-[4vh]">
          <Navbar />
          <Outlet />
        </main>
        <Footer />
      </div>
    </QueryClientProvider>
  );
}
