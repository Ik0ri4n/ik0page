import { execSync } from 'child_process';
import { writeFile } from 'fs';

export const getName = (path) => {
	const parts = path.split('/');
	const name = parts.pop()?.split('.').shift();

	if (!name) {
		throw new Error('Invalid blog path format');
	}
	return name;
};

export const getFolder = (path) => {
	const parts = path.split('/');
	const folder = parts[parts.length - 2];

	if (!folder) {
		throw new Error('Invalid blog path format');
	}
	return folder;
};

const added = execSync('git diff main --name-only --diff-filter=A src/content/blog/').toString(
	'utf-8'
);

for (const path of added.split('\n')) {
	if (path.length > 0) {
		const now = new Date();
		const date = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), 0, 0, 0, 0));
		const metadata = {
			date: date
		};

		writeFile(
			`src/lib/generated/blog/${getFolder(path)}/${getName(path)}.json`,
			JSON.stringify(metadata),
			(error) => {
				if (error) throw error;
			}
		);
	}
}
