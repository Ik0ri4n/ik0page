export default class Post {
	title: string;
	slug: string;
	#date: Date;
	excerpt: string;
	categories?: string | string[];

	constructor(title: string, slug: string, date: string, excerpt: string, categories: string | string[] | undefined = undefined) {
		this.title = title;
		this.slug = slug;
		this.#date = new Date(date);
		this.excerpt = excerpt;
		this.categories = categories;
	}

	get date(): string {
		return this.#date.toLocaleString('en-us', {
			month: 'long',
			day: 'numeric',
			year: 'numeric'
		});
	}
}
