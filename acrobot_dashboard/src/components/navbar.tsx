import { ModeToggle } from '@/components/mode-toggle';
import { RobotIcon } from '@/components/robot-icon';
import { useWsStatus } from '@/hooks/useWsStatus';
import { cn } from '@/lib/utils';

export function Navbar() {
  const { isConnected } = useWsStatus();

  return (
    <nav className="sticky top-0 mb-1 flex items-center justify-between border-b border-gray-200 bg-gray-50 p-2 dark:border-gray-50 dark:bg-stone-900 dark:text-white">
      <RobotIcon />
      <div className="relative cursor-pointer rounded-lg rounded-tr-none px-4 py-2 transition-all hover:bg-gray-100/10">
        <div
          className={cn(
            'absolute -right-0.5 -top-0.5 size-2 rounded-full',
            isConnected() ? 'bg-green-300' : 'bg-red-300'
          )}
        ></div>
        <div
          className={cn(
            'absolute -right-0.5 -top-0.5 size-2 animate-ping rounded-full',
            isConnected() ? 'bg-green-300' : 'bg-red-300'
          )}
        ></div>
        <p>ACROBOT</p>
      </div>
      <ModeToggle />
    </nav>
  );
}
