import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { cn } from '@/lib/utils';
import axios from 'axios';
import { File as FileIcon, Folder, Loader2, Play, RotateCw, ArrowRightToLine, Save, StopCircle } from 'lucide-react';
import { useEffect, useRef, useState } from 'react';
import { useQuery } from 'react-query';
import { useNavigate } from 'react-router-dom';

export function SD() {
  const navigate = useNavigate();
  const [filePath, setFilePath] = useState('/');
  const [path, setPath] = useState('/');
  const [power, setPower] = useState(50);

  const sdContents = useQuery(['sd_contents', path], ({ queryKey }) => fetchSDContents(queryKey[1]), {
    staleTime: 600_000,
    retryDelay: 60_000,
    keepPreviousData: true
  });
  const fileContents = useQuery(['file_contents', filePath], ({ queryKey }) => getFile(queryKey[1]));

  const [isSaving, setIsSaving] = useState(false);
  const pre = useRef<HTMLPreElement>(null);

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

  if (sdContents.isError) return <div>Error: {`${sdContents.error}`}</div>;

  return (
    <div className="relative">
      <div className="flex max-w-[100vw] overflow-hidden">
        <div className="flex min-h-[100vh] min-w-[450px] flex-col gap-0">
          {sdContents.isLoading ? (
            <div className="flex h-full w-full animate-spin items-center justify-center">
              <Loader2 size={32} />
            </div>
          ) : (
            sdContents.data?.map(({ type, name }) => {
              return (
                <div
                  key={type + name}
                  className={cn(
                    filePath === name ? 'bg-stone-500' : '',
                    'flex items-center border-b-2 border-gray-700 pl-1 transition-all hover:bg-stone-400'
                  )}
                >
                  <div
                    onClick={() => (type === 'dir' ? setPath(name) : setFilePath(name))}
                    className="flex flex-grow items-center hover:cursor-pointer"
                  >
                    <div className="flex items-center overflow-x-auto py-1">
                      {type === 'dir' ? <Folder size={26} /> : <FileIcon size={26} />}{' '}
                      <span className="ml-2">{name}</span>
                    </div>
                  </div>
                  <div
                    className={cn(
                      type === 'dir' ? 'pointer-events-none invisible' : '',
                      'flex h-[100%] items-center justify-center bg-stone-800'
                    )}
                  >
                    <Play
                      onClick={() => playFile({ file: name, mode: 'once', power })}
                      size={32}
                      className="h-[100%] p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                    />
                    <RotateCw
                      onClick={() => playFile({ file: name, mode: 'repeat', power })}
                      size={32}
                      className="h-[100%] p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                    />
                    <ArrowRightToLine
                      onClick={() => playFile({ file: name, mode: 'beginPosOnly', power })}
                      size={32}
                      className="h-[100%] p-1 transition-all hover:cursor-pointer hover:bg-stone-600"
                    />
                  </div>
                </div>
              );
            })
          )}
        </div>
        <div
          className={cn(
            sdContents.isLoading || fileContents.isLoading ? 'overflow-y-hidden' : 'overflow-y-auto',
            'relative flex-grow overflow-x-auto border-l-2 border-gray-200'
          )}
        >
          <div className="sticky left-0">
            <div className="mb-1 flex items-center justify-between gap-2 bg-stone-600 p-2">
              <div className="flex items-center gap-2">
                <Label htmlFor="power">Power</Label>
                <Input
                  type="number"
                  value={power}
                  onChange={(e) => setPower(parseFloat(e.target.value))}
                  step={0.01}
                  min={0}
                  max={100}
                  id="power"
                  onFocus={(e) => e.target.select()}
                />
              </div>
              <StopCircle
                className="transition-all hover:cursor-pointer hover:fill-red-300 hover:stroke-red-600"
                size={32}
                onClick={stop}
              />
            </div>
            <div className="relative bg-gray-700 p-2 text-center">
              <h3>{filePath}</h3>
              <div className="absolute right-2 top-1">
                {isSaving ? (
                  <Loader2 className="pointer-events-none animate-spin" size={32} />
                ) : (
                  <Save
                    className="hover:cursor-pointer"
                    onClick={async () => {
                      const fileContents = pre.current?.innerText;
                      if (!fileContents) return;
                      setIsSaving(true);
                      try {
                        await saveChanges({ fileName: filePath, fileContents });
                      } finally {
                        setIsSaving(false);
                      }
                    }}
                    size={32}
                  />
                )}
              </div>
            </div>
          </div>
          {sdContents.isLoading || fileContents.isLoading ? (
            <div className="flex h-full w-full animate-spin items-center justify-center">
              <Loader2 size={32} />
            </div>
          ) : (
            <pre ref={pre} contentEditable="plaintext-only" className="m-2 focus-visible:outline-none">
              {fileContents.data ? fileContents.data : null}
            </pre>
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

  return files.sort((a, b) => {
    if (a.type !== b.type) return a.type === 'dir' ? -1 : 1;

    return a.name.toLowerCase().localeCompare(b.name.toLowerCase());
  });
}

async function getFile(path: string): Promise<string> {
  const { data } = await axios.get<string>('/file', {
    baseURL: 'http://acrobot.local',
    params: { path }
  });

  return data;
}

export type PlayFileOptions = {
  file: string;
  mode: 'once' | 'repeat' | 'beginPosOnly';
  power: number;
};

async function playFile({ file, mode, power }: PlayFileOptions) {
  await axios.post(
    '/play',
    { file, mode, power },
    {
      baseURL: 'http://acrobot.local',
      headers: { 'Content-Type': 'application/json' }
    }
  );
}

export type SaveChangeOptions = {
  fileName: string;
  fileContents: string;
};

async function saveChanges({ fileName, fileContents }: SaveChangeOptions) {
  const form = new FormData();
  form.append('data', new File([new Blob([fileContents])], fileName));
  await axios.post('/edit', form, { baseURL: 'http://acrobot.local' });
}

async function stop() {
  await axios.post('/stop', {}, { baseURL: 'http://acrobot.local' });
}
