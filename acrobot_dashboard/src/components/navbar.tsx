import { ModeToggle } from '@/components/mode-toggle';
import { RobotIcon } from '@/components/robot-icon';

export function Navbar() {
  return (
    <nav className="sticky top-0 mb-1 flex items-center justify-between border-b border-gray-200 bg-gray-50 p-2 dark:border-gray-50 dark:bg-stone-900 dark:text-white">
      <RobotIcon />
      ACROBOT
      <ModeToggle />
    </nav>
  );
}
