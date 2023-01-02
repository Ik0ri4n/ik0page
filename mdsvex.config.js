import rehypeSlug from 'rehype-slug';
import rehypeAutolinkHeadings from 'rehype-autolink-headings';

const config = {
	extensions: ['.svelte.md', '.md', '.svx'],

	rehypePlugins: [rehypeSlug, rehypeAutolinkHeadings]
};

export default config;
