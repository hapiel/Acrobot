import { Button } from '@/components/ui/button';
import { acrobot } from '@/lib/AcroborAPI';
import { useState } from 'react';

function App() {
  const [response, setResponse] = useState('');

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-2">
      <div className="flex flex-col">
        <Button variant={'outline'} onClick={async () => setResponse(await acrobot.getStatus())}>
          Get status
        </Button>
      </div>
    </main>
  );
}

export default App;
