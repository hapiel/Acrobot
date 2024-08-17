import { cn } from '@/lib/utils';
import { File, Folder, Loader2, Play, Repeat, RotateCcw } from 'lucide-react';
import { useQuery } from 'react-query';

export function SD() {
  const { isLoading, isError, data, error } = useQuery('sd_contents', fetchSDContents);

  if (isLoading) {
    return (
      <div className="flex items-center justify-center">
        <Loader2 className="animate-spin" />
      </div>
    );
  }

  if (isError) return <div>Error: {`${error}`}</div>;

  return (
    <>
      <h1 className="mb-3 text-center text-3xl font-bold">SD card contents:</h1>
      <div className="flex flex-col gap-1 px-2">
        {data?.map(({ type, name }) => {
          return (
            <div
              key={type + name}
              className="flex justify-between gap-2 rounded-md border-2 border-gray-700 p-1 hover:bg-stone-600/40"
            >
              <div className="flex items-center justify-center gap-2">
                {type === 'dir' ? <Folder /> : <File />} {name}
              </div>
              <div
                className={cn(
                  'flex items-center justify-center gap-2',
                  type === 'dir' ? 'pointer-events-none invisible' : ''
                )}
              >
                <Play size={30} className="rounded-md p-1 transition-all hover:cursor-pointer hover:bg-stone-600" />
                <Repeat size={30} className="rounded-md p-1 transition-all hover:cursor-pointer hover:bg-stone-600" />
                <RotateCcw
                  size={30}
                  className="rounded-md p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                />
              </div>
            </div>
          );
        })}
      </div>
    </>
  );
}

async function fetchSDContents(): Promise<{ type: string; name: string }[]> {
  const response = await fetch('http://acrobot.local:3000/sd');
  if (!response.ok) throw new Error(`Failed to fetch SD contents: ${response.statusText}`);

  return await response.json();
}
