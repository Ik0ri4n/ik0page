import type Post from '$lib/types/post';
import type { SvelteComponent, SvelteComponentTyped } from 'svelte';

let slugToPath: Map<string, {folder: string, file: string}>;

const getSlug = (path: string): string => {
	const slug = path.split('/').pop()?.split('.').shift()?.split('_').pop();

	if (!slug) {
		throw new Error('Invalid blog path format');
	}
	return slug;
};

export const getPost = async (
	slug: string
): Promise<{
	Content: SvelteComponent;
	meta: Post;
}> => {
	if (!slugToPath) {
		slugToPath = new Map();
		const paths = import.meta.glob('../../content/blog/**/*.md', { as: 'url' });
		Object.keys(paths).forEach(async (path) => {
			const regex = /.*\/content\/blog\/([0-9]{4})\/(.+)\.md/;
			const parts = path.match(regex);
			if(!parts) {
				throw new Error("Invalid post path format");
			}
			
			slugToPath.set(getSlug(path), {folder:parts[1], file: parts[2]});
		});
	}

	const parts = slugToPath.get(slug);
	if (!parts) {
		throw new Error('Invalid slug for posts');
	}
	const {folder, file} = parts;
	
	const {default: Content, metadata} = await import(`../../content/blog/${folder}/${file}.md`);
	return {
		Content: Content,
		meta: {
			title: metadata.title,
			slug: slug,
			date: metadata.date,
			excerpt: metadata.excerpt,
			categories: metadata.categories || {}
		} satisfies Post
	};
};

export const getPosts = async (): Promise<Post[]> => {
	const mdModules = import.meta.glob<typeof import('*.md')>('../../content/blog/**/*.md');

	const posts = await Promise.all(
		Object.keys(mdModules).map(async (path) => {
			const { metadata } = await mdModules[path]();

			return {
				title: metadata.title,
				slug: getSlug(path),
				date: metadata.date,
				excerpt: metadata.excerpt,
				categories: metadata.categories || {}
			} satisfies Post;
		})
	);

	const sortedPosts = posts.sort((a, b) => Number(new Date(b.date)) - Number(new Date(a.date)));

	return sortedPosts;
};
