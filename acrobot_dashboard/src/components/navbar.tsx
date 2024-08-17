import { ModeToggle } from '@/components/mode-toggle';
import { RobotIcon } from '@/components/robot-icon';
import { Link } from 'react-router-dom';

export function Navbar() {
  return (
    <nav className="sticky top-0 mb-1 flex items-center justify-between border-b border-gray-200 bg-gray-50 p-2 dark:border-gray-50 dark:bg-stone-900 dark:text-white">
      <Link to={`/`}>
        <RobotIcon />
      </Link>
      <div className="flex items-center justify-center gap-2 *:rounded-md *:bg-stone-700 *:p-2 *:transition-all *:hover:cursor-pointer">
        <Link className="hover:bg-stone-500" to={`/`}>
          Robot Status
        </Link>
        <Link className="hover:bg-stone-600" to={`/sd`}>
          SD card
        </Link>
        <ModeToggle />
      </div>
    </nav>
  );
}
