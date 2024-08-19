import { cn } from '@/lib/utils';
import axios from 'axios';
import { File, Folder, Loader2, Play, Repeat, RotateCcw } from 'lucide-react';
import { useEffect, useState } from 'react';
import { useQuery } from 'react-query';
import { useNavigate } from 'react-router-dom';

export function SD() {
  const navigate = useNavigate();
  const [fileName, setFileName] = useState('/');
  const [path, setPath] = useState('/');

  const sdContents = useQuery(['sd_contents', path], ({ queryKey }) => fetchSDContents(queryKey[1]), {
    staleTime: 600_000,
    retryDelay: 60_000,
    keepPreviousData: true
  });
  const fileContents = useQuery(['file_contents', fileName], ({ queryKey }) => fetchFileContents(queryKey[1]));

  useEffect(() => {
    const handlePopState = (event: PopStateEvent) => {
      event?.preventDefault();

      if (path === '/') {
        navigate(-1);
        return;
      }

      navigate('/sd');
      const newPath = path.split('/').slice(0, -1).join('/');
      setPath(newPath ? newPath : '/');
    };
    navigate('/sd');
    window.addEventListener('popstate', handlePopState);

    return () => window.removeEventListener('popstate', handlePopState);
  }, [navigate, path]);

  if (sdContents.isLoading) {
    return (
      <div className="flex items-center justify-center">
        <Loader2 className="animate-spin" />
      </div>
    );
  }

  if (sdContents.isError) return <div>Error: {`${sdContents.error}`}</div>;

  return (
    <div className="relative">
      <div className="flex max-w-[100vw] overflow-hidden">
        <div className="flex flex-col gap-0">
          {sdContents.data?.map(({ type, name }) => {
            return (
              <div
                key={type + name}
                className={cn(
                  fileName === name ? 'bg-stone-500' : '',
                  'flex justify-between gap-2 border-b-2 border-gray-700 pl-1 transition-all hover:cursor-pointer hover:bg-stone-400'
                )}
                onClick={() => (type === 'dir' ? setPath(name) : setFileName(name))}
              >
                <div className="flex items-center justify-center gap-2">
                  {type === 'dir' ? <Folder /> : <File />} {name}
                </div>
                <div
                  className={cn(
                    'flex items-center justify-center gap-2 bg-stone-800 p-1',
                    type === 'dir' ? 'pointer-events-none invisible' : ''
                  )}
                >
                  <Play
                    onClick={() => playFile({ file: name, mode: 'once', power: 50 })}
                    size={30}
                    className="rounded-md p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                  />
                  <Repeat
                    onClick={() => playFile({ file: name, mode: 'repeat', power: 50 })}
                    size={30}
                    className="rounded-md p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                  />
                  <RotateCcw
                    onClick={() => playFile({ file: name, mode: 'beginPosOnly', power: 50 })}
                    size={30}
                    className="rounded-md p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                  />
                </div>
              </div>
            );
          })}
        </div>
        <div className="relative max-w-[100%] flex-grow overflow-x-auto border-l-2 border-gray-200">
          <h3 className="sticky left-0 bg-gray-700 p-2 text-center">{fileName}</h3>
          {fileContents.isLoading ? (
            <Loader2 className="animate-spin" />
          ) : (
            <pre className="m-2">{fileContents.data ? fileContents.data : null}</pre>
          )}
        </div>
      </div>
    </div>
  );
}

export type GetSdResponse = { type: string; name: string }[];

async function fetchSDContents(dir: string): Promise<GetSdResponse> {
  const files: GetSdResponse = [];
  const { data } = await axios.get<GetSdResponse>(`/list`, {
    baseURL: 'http://acrobot.local',
    params: { dir }
  });

  files.push(...data);

  return files;
}

async function fetchFileContents(fileName: string): Promise<string> {
  const { data } = await axios.get<string>(`${fileName}`, {
    baseURL: 'http://acrobot.local'
  });

  return data;
}

export type PlayFileOptions = {
  file: string;
  mode: 'once' | 'repeat' | 'beginPosOnly';
  power: number;
};
async function playFile({ file, mode, power }: PlayFileOptions) {
  await axios.get('/play', {
    baseURL: 'http://acrobot.local',
    params: { file, mode, power }
  });
}
