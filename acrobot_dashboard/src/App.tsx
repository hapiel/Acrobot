import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { useState } from 'react';

function App() {
  const [command, setCommand] = useState('');
  const [response, setResponse] = useState('');

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-12">
      <div className="flex flex-col gap-4">
        <label htmlFor="command">Command</label>
        <Input
          type="text"
          className="rounded-md"
          id="command"
          name="command"
          value={command}
          onChange={(e) => setCommand(e.target.value)}
        />
        <Button
          variant={'outline'}
          onClick={async () => {
            const command = document.getElementById('command') as HTMLInputElement;
            const response = await fetch('http://acrobot.local/test-command', {
              method: 'POST',
              body: JSON.stringify({ command: command.value })
            });
            const json = await response.json();
            console.log(json);
            setResponse(JSON.stringify(json, null, 2));
          }}
        >
          Send test command
        </Button>
        <textarea className="h-60 w-96 resize-none rounded-md" value={response}></textarea>
      </div>
    </main>
  );
}

export default App;
