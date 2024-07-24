export function RobotIcon() {
  return (
    <svg
      className="size-12 stroke-black dark:fill-white dark:stroke-white"
      width="200"
      height="200"
      viewBox="0 0 200 200"
      xmlns="http://www.w3.org/2000/svg"
    >
      <rect x="35" y="35" width="130" height="130" rx="10" ry="10" fill="none" stroke-width="2" />

      <circle cx="75" cy="75" r="10" stroke-width="2" />
      <circle cx="125" cy="75" r="10" stroke-width="2" />

      <path d="M65,120 Q100,145 135,120" fill="none" stroke-width="3" />

      <line x1="100" y1="15" x2="100" y2="35" stroke-width="2" />
      <circle cx="100" cy="15" r="7" fill="none" stroke-width="2" />
    </svg>
  );
}
