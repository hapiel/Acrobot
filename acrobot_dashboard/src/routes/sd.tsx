import { Button } from '@/components/ui/button';
import { DialogClose, DialogDescription, DialogFooter, DialogHeader } from '@/components/ui/dialog';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { cn } from '@/lib/utils';
import { Dialog, DialogTrigger, DialogContent, DialogTitle } from '@/components/ui/dialog';
import axios from 'axios';
import { File as FileIcon, Folder, Loader2, Play, RotateCw, ArrowRightToLine, Save, StopCircle } from 'lucide-react';
import { Dispatch, SetStateAction, useEffect, useRef, useState } from 'react';
import { useQuery } from 'react-query';
import { useNavigate } from 'react-router-dom';

export function SD() {
  const navigate = useNavigate();
  const [filePath, setFilePath] = useState('/');
  const [path, setPath] = useState('/');
  const [power, setPower] = useState(50);
  const [fileContents, setFileContents] = useState('');

  const sdContentsQuery = useQuery(['sd_contents', path], ({ queryKey }) => fetchSDContents(queryKey[1]), {
    staleTime: 600_000,
    retryDelay: 60_000,
    keepPreviousData: true,
    onSuccess: (data) => {
      const firstNonDir = data.find((e) => e.type !== 'dir');
      if (!firstNonDir) {
        setFilePath('');
        setFileContents('');
        return;
      }

      setFilePath(firstNonDir.name);
    }
  });
  const fileContentsQuery = useQuery(['file_contents', filePath], ({ queryKey }) => getFile(queryKey[1]), {
    onSuccess: (data) => setFileContents(data)
  });

  const [isSaving, setIsSaving] = useState(false);

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

  if (sdContentsQuery.isError) return <div>Error: {`${sdContentsQuery.error}`}</div>;

  return (
    <div className="relative flex flex-1 flex-col overflow-hidden">
      <div className="flex flex-1 overflow-hidden">
        <div className="flex min-w-[600px] flex-shrink-0 flex-col gap-0 overflow-y-auto overflow-x-hidden">
          {sdContentsQuery.isFetching ? (
            <div className="flex h-full w-full animate-spin items-center justify-center">
              <Loader2 size={32} />
            </div>
          ) : (
            sdContentsQuery.data?.map(({ type, name }) => {
              return (
                <div
                  key={type + name}
                  className={cn(
                    filePath === name ? 'bg-stone-500' : '',
                    'flex items-center gap-2 border-b-2 border-gray-700 pl-1 transition-all hover:bg-stone-400'
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
            sdContentsQuery.isFetching || fileContentsQuery.isFetching ? 'overflow-y-hidden' : 'overflow-y-auto',
            'relative flex-grow overflow-x-auto border-l-2 border-gray-200'
          )}
        >
          <div className="sticky left-0 top-0 bg-black">
            <div className="mb-1 flex items-center justify-between gap-2 bg-stone-600 p-2">
              <UploadFile path={path} />
              <div className="flex-end flex items-center gap-4">
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
            </div>
            <div className="relative bg-gray-700 p-2 text-center">
              <h3 className="flex items-center justify-center">
                {sdContentsQuery.isFetching ? (
                  <Loader2 className="pointer-events-none animate-spin" size={24} />
                ) : (
                  filePath
                )}
              </h3>
              <div className="absolute right-2 top-1">
                {isSaving ? (
                  <Loader2 className="pointer-events-none animate-spin" size={32} />
                ) : (
                  <Save
                    className="hover:cursor-pointer"
                    onClick={async () => {
                      if (!fileContents) return;
                      setIsSaving(true);
                      try {
                        await saveFile({ fileName: filePath, fileContents });
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
          {sdContentsQuery.isFetching || fileContentsQuery.isFetching ? (
            <div className="flex h-full w-full animate-spin items-center justify-center">
              <Loader2 size={32} />
            </div>
          ) : (
            <FileEditor fileContents={fileContents} setFileContents={setFileContents} />
          )}
        </div>
      </div>
    </div>
  );
}

export type GetSdResponse = { type: string; name: string }[];

async function fetchSDContents(dir: string): Promise<GetSdResponse> {
  const files: GetSdResponse = [];
  console.log(`fetching ${dir}`);
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

async function saveFile({ fileName, fileContents }: SaveChangeOptions) {
  const form = new FormData();
  form.append('data', new File([new Blob([fileContents])], fileName));
  await axios.post('/edit', form, { baseURL: 'http://acrobot.local' });
}

async function stop() {
  await axios.post('/stop', {}, { baseURL: 'http://acrobot.local' });
}

type UploadFileProps = {
  path: string;
};

function UploadFile({ path }: UploadFileProps) {
  const [destination, setDestination] = useState<string>(path);
  const [isLoading, setIsLoading] = useState(false);
  const [file, setFile] = useState<File | null>(null);
  const [isOpen, setIsOpen] = useState(false);

  useEffect(() => setDestination(path), [path]);

  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const files = event.target.files;
    if (files && files.length > 0) {
      setFile(files[0]);
    }
  };

  const handleUpload = async () => {
    if (!file) {
      return;
    }

    setIsLoading(true);

    try {
      const fileContents = await readFileContents(file);

      console.log('File contents:', fileContents);
      console.log('Destination:', destination);

      await saveFile({ fileName: `${destination.replace(/\/*$/, '')}/${file.name}`, fileContents });
    } catch (error) {
      console.error('Error reading or uploading file:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const readFileContents = (file: File): Promise<string> => {
    return new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = (event) => {
        if (event.target) {
          resolve(event.target.result as string);
        } else {
          reject(new Error('Failed to read file'));
        }
      };
      reader.onerror = (error) => reject(error);
      reader.readAsText(file);
    });
  };

  return (
    <Dialog open={isOpen} onOpenChange={(open) => setIsOpen(open)}>
      <DialogTrigger asChild>
        <Button variant="outline" onClick={() => setIsOpen(true)}>
          Upload file
        </Button>
      </DialogTrigger>
      <DialogContent className="sm:max-w-md">
        <DialogHeader>
          <DialogTitle>Upload file</DialogTitle>
          <DialogDescription>Upload a file to the specified destination</DialogDescription>
        </DialogHeader>
        <div className="flex items-center space-x-2">
          <div className="grid flex-1 gap-2">
            <Label htmlFor="destination">Destination</Label>
            <Input
              id="destination"
              disabled={isLoading}
              value={destination}
              onInput={(e) => setDestination(e.currentTarget.value)}
            />
          </div>
        </div>
        <div className="flex items-center space-x-2">
          <div className="grid flex-1 gap-2">
            <Label htmlFor="file">File</Label>
            <Input
              id="file"
              type="file"
              disabled={isLoading}
              onChange={handleFileChange}
              className="file:text-white hover:cursor-pointer file:hover:cursor-pointer"
            />
          </div>
        </div>
        <DialogFooter className="sm:justify-start">
          <DialogClose asChild>
            <Button disabled={isLoading} type="button" variant="secondary">
              Close
            </Button>
          </DialogClose>
          <Button type="submit" variant="default" disabled={isLoading} onClick={handleUpload}>
            Upload
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}

type FileEditorProps = {
  fileContents: string;
  setFileContents: Dispatch<SetStateAction<string>>;
};

function FileEditor({ fileContents, setFileContents }: FileEditorProps) {
  const textareaRef = useRef<HTMLTextAreaElement>(null);

  useEffect(() => {
    if (textareaRef.current) {
      textareaRef.current.value = fileContents;
    }
  }, [fileContents]);

  const handleChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    setFileContents(e.target.value);
  };

  useEffect(() => {
    const adjustTextareaHeight = () => {
      if (textareaRef.current) {
        textareaRef.current.style.height = 'auto';
        textareaRef.current.style.height = `${textareaRef.current.scrollHeight}px`;
      }
    };

    adjustTextareaHeight();
    window.addEventListener('resize', adjustTextareaHeight);

    return () => {
      window.removeEventListener('resize', adjustTextareaHeight);
    };
  }, [fileContents]);

  return (
    <textarea
      ref={textareaRef}
      value={fileContents}
      onChange={handleChange}
      className="h-full w-full resize-none border-none bg-gray-800 p-2 font-mono text-sm text-white focus:outline-none"
      spellCheck={false}
    />
  );
}
